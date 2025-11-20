// Pages/BoardPage.jsx
import React, { useState } from 'react'
import {
  Box,
  Container,
  Typography,
  Paper,
  Stack,
  Chip,
  Button,
  Divider,
  Dialog,
  DialogTitle,
  DialogContent,
  DialogActions,
  TextField,
  MenuItem,
  Select,
  FormControl,
  InputLabel,
  // Checkbox removed per UI change
} from '@mui/material'
import EditIcon from '@mui/icons-material/Edit'
import DeleteIcon from '@mui/icons-material/Delete'
import PushPinIcon from '@mui/icons-material/PushPin'
import CalendarTodayIcon from '@mui/icons-material/CalendarToday'
import ArrowBackIcon from '@mui/icons-material/ArrowBack'
import { useNavigate } from 'react-router-dom'
import { Navigate } from 'react-router-dom'
  import { useGlobal } from '../SessionManager'
  import '../App.css'
import { addTask, updateTask, deleteTask, removeUserFromBoard, deleteBoard } from '../APIManager'
import { DatePicker } from "@mui/x-date-pickers/DatePicker";
import { LocalizationProvider } from "@mui/x-date-pickers/LocalizationProvider";
import { AdapterDayjs } from "@mui/x-date-pickers/AdapterDayjs";
import dayjs from "dayjs";

  export default function BoardPage() {
    const navigate = useNavigate()
    const { state, updateState } = useGlobal()
    const board = state.currentTaskBoard

    const [activePulse, setActivePulse] = useState(false)
    //const [pinned, setPinned] = useState(() => new Set((board?.tasks || []).filter(t => t.isPinned).map(t => t.id)))

    const [editOpen, setEditOpen] = useState(false)
    const [editingTask, setEditingTask] = useState(null)
    const [addOpen, setAddOpen] = useState(false)
    const [newTask, setNewTask] = useState({ title: '', type: '', start_date: '', due_date: '', assignedTo: '' })

    
    if (!state.loggedIn) {
      return <Navigate to="/login" replace />
    }

    if (!board) {
      return (
        <Box className="board-page-root">
          <Container maxWidth="lg" className="board-page-container">
            <Box className="board-hero">
              <Typography variant="h4" className="pixel-text">
                No board selected
              </Typography>
              <Typography variant="body1" className="board-description">
                Please go back to the dashboard and choose a task board.
              </Typography>
              <Box sx={{ mt: 3 }}>
                <Button variant="contained" onClick={() => navigate('/') }>
                  Back to dashboard
                </Button>
              </Box>
            </Box>
          </Container>
        </Box>
      )
    }

    let boards = board.isShared ? state.sharedTaskboardList : state.privateTaskboardList;
    let boardIndex = -1;
    for (let i = 0; i < boards.length; i++){
      if (boards[i].taskboard_id == board.taskboard_id){
        boardIndex = i;
        break;
      }
    }

    function openEdit(task) {
      setEditingTask({
        task_id: task.task_id,
        title: task.title ?? '',
        type: task.type ?? '',
        due_date: task.due_date ?? '',
        start_date: task.start_date ?? '',
        assignedTo: task.assignedUser?.username ?? '',
      })
      setEditOpen(true)
    }

    function reformatTask(task){
      let nTask = task;

      if (task.assignedTo != '' && task.assignedTo != 'Unassigned'){
        for (let i = 0; i < board.users.length; i++){
          if (task.assignedTo === board.users[i].username){
            nTask.assigned_user_id = board.users[i].userId;
            nTask.assignedUser = {
              userId: board.users[i].userId,
              username: board.users[i].username
            }
            break;
          }
        }
      }

      //nTask.due_date = nTask.due_date.format("YYYY/MM/DD")
      //nTask.start_date = nTask.start_date.format("YYYY/MM/DD")
      return nTask;
    }

    function updateTaskCallBack(nTask, data){
      if (data){
          let nBoard = board;
          
          for (let i = 0; i < nBoard.tasks.length; i++){
            if (nBoard.tasks[i].task_id === nTask.task_id){
              nBoard.tasks[i] = nTask;
              break;
            }
          }

          nBoard.tasks.sort(sort)
          
          updateState({currentTaskBoard: nBoard})
          let newList = board.isShared ? state.sharedTaskboardList : state.privateTaskboardList;
          newList[boardIndex] = board;
          if (board.isShared) updateState({sharedTaskboardList: newList})
          else updateState({privateTaskboardList: newList})

          setEditOpen(false)
          setEditingTask(null)
        }
    }

    function saveEdit() {
      let nTask = reformatTask(editingTask)

      updateTask(nTask, state.user.userId).then(data=>updateTaskCallBack(nTask, data))
    }

    function togglePin(task) {

      let nTask = task
      nTask.pinned = !nTask.pinned;

      updateTask(nTask, state.user.userId).then(data=>updateTaskCallBack(nTask, data))
    }

    function sort(a, b){
        if (a.pinned === b.pinned){
            if (a.due_date < b.due_date) return -1;
            else if (a.due_date > b.due_date) return 1;
            if (a.start_date < b.start_date) return -1;
            else if (a.start_date > b.start_date)return 1;

            return 0;
        }
        return a.pinned ? -1 : 1; 
    }

    function onDeleteTask(task){
      if (confirm("Are you sure you want to delete " + task.title + "?")){
        deleteTask(task.task_id, state.user.userId).then(data => {
          if (data){
            alert("successfully deleted task " + task.title);
            board.tasks = board.tasks.filter(tsk => tsk.task_id !== task.task_id);
            updateState({currentTaskBoard: board})
            let newList = board.isShared ? state.sharedTaskboardList : state.privateTaskboardList;
            newList[boardIndex] = board;
            if (board.isShared) updateState({sharedTaskboardList: newList})
            else updateState({privateTaskboardList: newList})
          }
        });
      }
    }

    function openAddTask(){
      setNewTask({ title: '', type:'', due_date: '', assignedTo: '', start_date: '' })
      setAddOpen(true)
    }

    function onAddTask() {
      let nTask = reformatTask(newTask)

      addTask(nTask, board.taskboard_id, state.user.userId).then(data=>{
        if (data != -1){
          nTask.task_id = data;
          let nBoard = board;
          nBoard.tasks.push(nTask);
          nBoard.tasks.sort(sort)
          
          updateState({currentTaskBoard: nBoard})
          let newList = board.isShared ? state.sharedTaskboardList : state.privateTaskboardList;
          newList[boardIndex] = board;
          if (board.isShared) updateState({sharedTaskboardList: newList})
          else updateState({privateTaskboardList: newList})

          setNewTask({ title: '', type:'', due_date: '', assignedTo: '', start_date: '' })
          setAddOpen(false)
        }
      })
    }

    function onDeleteBoard(){
      if (confirm("Are you sure you want to delete this taskbaord? All tasks will be deleted and all members will be forced to leave!")){
        deleteBoard(board.taskboard_id, state.user.userId).then(data => {
          if (data){
            let newList = board.isShared ? state.sharedTaskboardList : state.privateTaskboardList;
            newList.splice(boardIndex, 1);
            if (board.isShared) updateState({sharedTaskboardList: newList})
            else updateState({privateTaskboardList: newList})
            navigate('/')
          }
        })
      }
    }

    function onLeaving(){
      if (confirm("Are you sure you want to leave this taskbaord?")){
        removeUserFromBoard(state.users.userId, board.taskboard_id, state.users.userId).then(data => {
          if (data){
            navigate('/')
          }
        })
      }
    }

    // pulse ACTIVE indicator briefly when task count changes
    React.useEffect(() => {
      if (!board) return
      setActivePulse(true)
      const t = setTimeout(() => setActivePulse(false), 900)
      return () => clearTimeout(t)
    }, [board?.tasks?.length])

    const boardDescription = board.description ?? 'Level up your programming skills'

    // Assigned tasks: prefer task.assignedToMe if present, else first two
    const assigned = board.tasks.filter(t => t.assignedUser?.userId ?? -1 === state.user.userId)
     
    let isAdmin = false;
    for (let i = 0; i < board.users.length; i++){
      if (board.users[i].userId === state.user.userId){
        isAdmin = board.users[i].isAdmin
        break;
      }
    }

    const currentDate = new Date().toISOString().split('T')[0]

    return (
      <LocalizationProvider dateAdapter={AdapterDayjs}>
      <Box className="board-page-root" style={{ background: 'linear-gradient(#051029,#071028 200px)' }}>
        <Container maxWidth="lg" className="board-page-container">
          <div className="board-shell-container">
            <div className="dashboard-floating">
              <div className="fancy-btn" role="button" onClick={() => navigate('/') }>
                <div className="shadow" />
                <div className="edge" />
                <div className="front "><ArrowBackIcon fontSize="small" /> DASHBOARD</div>
              </div>
            </div>
            <Paper className="board-shell" elevation={4}>
            {/* Header */}
            <Stack direction="row" justifyContent="space-between" alignItems="center" sx={{ p: 2 }}>
              <Stack direction="row" spacing={2} alignItems="center">
                <Box>
                  <Typography className="pixel-text board-name">{board.name}</Typography>
                </Box>
              </Stack>

              <Stack direction="row" spacing={1} alignItems="center">
                {/* return to dashboard (floating button used) */}

                {/* active indicator (glowing text) */}
                <Typography className={`active-indicator-text pixel-text ${activePulse ? 'pulse' : ''}`} sx={{ mr: 1 }} aria-hidden>
                  {board.tasks.length} ACTIVE
                </Typography>

                {/* board-level controls moved to header - use fancy 3D markup */}
 { isAdmin &&                
                <div className="fancy-btn blue" role="button" onClick={() => console.log('manage users', board.taskboard_id)}>
                  <div className="shadow" />
                  <div className="edge" />
                  <div className="front">MANAGE USERS</div>
                </div>
  }
{ isAdmin && 
                <div className="fancy-btn red" role="button" onClick={onDeleteBoard}>
                  <div className="shadow" />
                  <div className="edge" />
                  <div className="front">DELETE BOARD</div>
                </div>
  }
  { board.isShared &&
                <div className="fancy-btn yellow" role="button" onClick={onLeaving}>
                  <div className="shadow" />
                  <div className="edge" />
                  <div className="front">LEAVE BOARD</div>
                </div>
  }
                { isAdmin && 
                <div className="fancy-btn green" role="button" onClick={() => openAddTask()}>
                  <div className="shadow" />
                  <div className="edge" />
                  <div className="front">ADD TASK</div>
                </div>
  }
              </Stack>
            </Stack>

            <Divider sx={{ borderColor: 'rgba(255,255,255,0.04)' }} />

            {/* Assigned to You */}
            {assigned.length !== 0 &&
            <Box sx={{ p: 2 }}>
              <Typography className="section-label">ASSIGNED TO YOU</Typography>
              <Stack spacing={1} sx={{ mt: 1 }}>
                {assigned.map((task) => (
                  <Paper key={task.task_id} className="assigned-card">
                    <Stack direction="row" alignItems="center" justifyContent="space-between" sx={{ width: '100%' }}>
                      <Stack direction="row" spacing={2} alignItems="center">
                        <Box>
                          <Typography className="pixel-text" sx={{ color: '#fff', fontWeight: 700 }}>{task.title || 'Untitled'}</Typography>
                          <Stack direction="row" spacing={1} sx={{ mt: 1 }}>
                            <Chip label={`${task.reward ?? 20} XP`} size="small" className="chip-reward" style={{ background: '#fff700ff', color: '#000000ff', fontWeight: 700, boxShadow: '0 6px 18px rgba(59,7,16,0.08)' }} />
                            <Chip icon={<CalendarTodayIcon />} label={task.due_date ?? 'TODAY'} size="small" className="chip-date" style={{ background: '#05f2fac1', color: '#fff', fontWeight: 700, boxShadow: '0 6px 18px rgba(29,78,216,0.08)' }} />
                            {task.pinned && <Chip label="PINNED" size="small" className="chip-pinned" style={{ background: '#5243e6', color: '#fff', fontWeight: 700, boxShadow: '0 6px 18px rgba(82,67,230,0.08)' }} />}
                            {task.start_date > currentDate && <Chip label={"Starting At " + task.start_date} size="small" style={{ background: '#5243e6', color: '#fff', fontWeight: 700, boxShadow: '0 6px 18px rgba(82,67,230,0.08)' }} />}
                          </Stack>
                        </Box>
                      </Stack>

                      <Stack direction="row" spacing={1} className="task-action-row">
                        <button className="button yellow" data-label={task.pinned ? 'Unpin' : 'Pin'} onClick={() => togglePin(task)}>
                          <span className="svgIcon"><PushPinIcon fontSize="small" /></span>
                        </button>
                        <button className="button cyan" data-label="Edit" onClick={() => openEdit(task)}>
                          <span className="svgIcon"><EditIcon fontSize="small" /></span>
                        </button>
                        { isAdmin && 
                        <button className="button red" data-label="Delete" onClick={() => onDeleteTask(task)}>
                          <span className="svgIcon"><DeleteIcon fontSize="small" /></span>
                        </button>
                        }
                      </Stack>
                    </Stack>
                  </Paper>
                ))}
              </Stack>
            </Box>
  }
            <Divider sx={{ my: 2, borderColor: 'rgba(255,255,255,0.04)' }} />

            {/* Quest Queue */}
            <Box sx={{ p: 2 }}>
              <Typography className="section-label">Task Queue</Typography>
              <Stack spacing={1} sx={{ mt: 1 }}>
                {board.tasks.map((task, index) => (
                  <Paper key={task.task_id} className="assigned-card">
                    <Stack direction="row" alignItems="center" justifyContent="space-between" sx={{ width: '100%' }}>
                      <Stack direction="row" spacing={2} alignItems="center">
                        <Box>
                          <Typography className="pixel-text" sx={{ color: '#fff', fontWeight: 700 }}>{task.title || 'Untitled'}</Typography>
                          <Stack direction="row" spacing={1} sx={{ mt: 1 }}>
                            <Chip label={`${task.reward ?? 20} XP`} size="small" className="chip-reward" style={{ background: '#fff700ff', color: '#000000ff', fontWeight: 700, boxShadow: '0 6px 18px rgba(59,7,16,0.08)' }} />
                            <Chip icon={<CalendarTodayIcon />} label={task.due_date ?? 'TODAY'} size="small" className="chip-date" style={{ background: '#05f2fac1', color: '#fff', fontWeight: 700, boxShadow: '0 6px 18px rgba(29,78,216,0.08)' }} />
                            {task.pinned && <Chip label="PINNED" size="small" className="chip-pinned" style={{ background: '#5243e6', color: '#fff', fontWeight: 700, boxShadow: '0 6px 18px rgba(82,67,230,0.08)' }} />}
                            {task.start_date > currentDate && <Chip label={"Starting At " + task.start_date} size="small" style={{ background: '#5243e6', color: '#fff', fontWeight: 700, boxShadow: '0 6px 18px rgba(82,67,230,0.08)' }} />}
                          </Stack>
                        </Box>
                      </Stack>

                      <Stack direction="row" spacing={1} className="task-action-row">
                        <button className="button yellow" data-label={task.pinned ? 'Unpin' : 'Pin'} onClick={() => togglePin(task)}>
                          <span className="svgIcon"><PushPinIcon fontSize="small" /></span>
                        </button>
                        <button className="button cyan" data-label="Edit" onClick={() => openEdit(task)}>
                          <span className="svgIcon"><EditIcon fontSize="small" /></span>
                        </button>
                        { isAdmin && 
                        <button className="button red" data-label="Delete" onClick={() => onDeleteTask(task)}>
                          <span className="svgIcon"><DeleteIcon fontSize="small" /></span>
                        </button>
                        }
                      </Stack>
                    </Stack>
                  </Paper>
                ))}
              </Stack>
            </Box>

            {/* Edit dialog */}
            <Dialog open={editOpen} onClose={() => setEditOpen(false)}>
              <DialogTitle>Edit Task</DialogTitle>
              <DialogContent>
                <TextField fullWidth label="Title" sx={{ mt: 1 }} value={editingTask?.title ?? ''} onChange={(e) => setEditingTask({...editingTask, title: e.target.value})} />
                <TextField fullWidth label="Type or description" sx={{ mt: 1 }} value={editingTask?.type ?? ''} onChange={(e) => setEditingTask({...editingTaskTask, type: e.target.value})} />
                <DatePicker slotProps={{textField: {fullWidth: true}}} label="Start date" sx={{ mt: 2 }} value={dayjs(editingTask?.start_date) ?? null} onChange={(e) => setEditingTask({...editingTask, start_date: e.format("YYYY/MM/DD")})} />
                <DatePicker slotProps={{textField: {fullWidth: true}}} label="Due date" sx={{ mt: 2 }} value={dayjs(editingTask?.due_date) ?? null} onChange={(e) => setEditingTask({...editingTask, due_date: e.format("YYYY/MM/DD")})} />
                <FormControl fullWidth sx={{ mt: 2 }}>
                  <InputLabel id="assign-label">Assign to</InputLabel>
                  <Select labelId="assign-label-add" value={editingTask?.assignedTo ?? ''} label="Assign to" onChange={(e) => setEditingTask({...editingTask, assignedTo: e.target.value})}>
                    <MenuItem value='Unassigned'>Unassigned</MenuItem>
                    {board.users.map((user, index) => (
                      <MenuItem value={user.username}>{user.username}</MenuItem>
                    ))}
                  </Select>
                </FormControl>
              </DialogContent>
              <DialogActions>
                <Button onClick={() => setEditOpen(false)}>Cancel</Button>
                <Button onClick={saveEdit} variant="contained">Save</Button>
              </DialogActions>
            </Dialog>

            {/* Add Task dialog (separate) */}
            <Dialog open={addOpen} onClose={() => setAddOpen(false)}>
              <DialogTitle>Add Task</DialogTitle>
              <DialogContent>
                <TextField fullWidth label="Title" sx={{ mt: 1 }} value={newTask.title} onChange={(e) => setNewTask({...newTask, title: e.target.value})} />
                <TextField fullWidth label="Type or description" sx={{ mt: 1 }} value={newTask.type} onChange={(e) => setNewTask({...newTask, type: e.target.value})} />
                {/*<TextField fullWidth label="Start date" sx={{ mt: 2 }} value={newTask.start_date} onChange={(e) => setNewTask({...newTask, start_date: e.target.value})} />*/}
                <DatePicker slotProps={{textField: {fullWidth: true}}} label="Start date" sx={{ mt: 2 }} value={dayjs(newTask.start_date)} onChange={(e) => setNewTask({...newTask, start_date: e.format("YYYY/MM/DD")})}/>
                <DatePicker slotProps={{textField: {fullWidth: true}}} label="Due date" sx={{ mt: 2 }} value={dayjs(newTask.due_date)} onChange={(e) => setNewTask({...newTask, due_date: e.format("YYYY/MM/DD")})} />
                <FormControl fullWidth sx={{ mt: 2 }}>
                  <InputLabel id="assign-label-add">Assign to</InputLabel>
                  <Select labelId="assign-label-add" value={newTask.assignedTo} label="Assign to" onChange={(e) => setNewTask({...newTask, assignedTo: e.target.value})}>
                    <MenuItem value='Unassigned'>Unassigned</MenuItem>
                    {board.users.map((user, index) => (
                      <MenuItem value={user.username}>{user.username}</MenuItem>
                    ))}
                  </Select>
                </FormControl>
              </DialogContent>
              <DialogActions>
                <Button onClick={() => setAddOpen(false)}>Cancel</Button>
                <Button onClick={onAddTask} variant="contained">Create</Button>
              </DialogActions>
            </Dialog>
          </Paper>
          </div>
        </Container>
      </Box>
      </LocalizationProvider>
    )
  }