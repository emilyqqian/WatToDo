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

  export default function BoardPage() {
    const navigate = useNavigate()
    const { state } = useGlobal()
    const board = state.currentTaskBoard

  const [activePulse, setActivePulse] = useState(false)
  const [pinned, setPinned] = useState(() => new Set((board?.tasks || []).filter(t => t.isPinned).map(t => t.id)))

    const [editOpen, setEditOpen] = useState(false)
    const [editingTask, setEditingTask] = useState(null)
    const [addOpen, setAddOpen] = useState(false)
    const [newTask, setNewTask] = useState({ title: '', due_date: '', assignedTo: '' })

    function openEdit(task) {
      setEditingTask({
        id: task.id,
        title: task.title ?? '',
        due_date: task.due_date ?? '',
        assignedTo: task.assignedTo ?? '',
      })
      setEditOpen(true)
    }

    function saveEdit() {
      console.log('save edit', editingTask)
      setEditOpen(false)
      setEditingTask(null)
    }

    function togglePin(id) {
      setPinned(prev => {
        const next = new Set(prev)
        if (next.has(id)) next.delete(id)
        else next.add(id)
        return next
      })
      console.log('toggle pin', id)
    }

    function addTask() {
  setNewTask({ title: '', due_date: '', assignedTo: '' })
  setAddOpen(true)
  console.log('open add task dialog')
    }

    // pulse ACTIVE indicator briefly when task count changes
    React.useEffect(() => {
      if (!board) return
      setActivePulse(true)
      const t = setTimeout(() => setActivePulse(false), 900)
      return () => clearTimeout(t)
    }, [board?.tasks?.length])

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

    const boardDescription = board.description ?? 'Level up your programming skills'

    // Assigned tasks: prefer task.assignedToMe if present, else first two
    const assigned = board.tasks.filter(t => t.assignedToMe).length
      ? board.tasks.filter(t => t.assignedToMe)
      : board.tasks.slice(0, 2)

    return (
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
                <div className="fancy-btn blue" role="button" onClick={() => console.log('manage users', board.id)}>
                  <div className="shadow" />
                  <div className="edge" />
                  <div className="front">MANAGE USERS</div>
                </div>

                <div className="fancy-btn red" role="button" onClick={() => console.log('delete board', board.id)}>
                  <div className="shadow" />
                  <div className="edge" />
                  <div className="front">DELETE BOARD</div>
                </div>

                <div className="fancy-btn yellow" role="button" onClick={() => console.log('leave board', board.id)}>
                  <div className="shadow" />
                  <div className="edge" />
                  <div className="front">LEAVE BOARD</div>
                </div>
                <div className="fancy-btn green" role="button" onClick={() => addTask()}>
                  <div className="shadow" />
                  <div className="edge" />
                  <div className="front">ADD TASK</div>
                </div>
              </Stack>
            </Stack>

            <Divider sx={{ borderColor: 'rgba(255,255,255,0.04)' }} />

            {/* Assigned to You */}
            <Box sx={{ p: 2 }}>
              <Typography className="section-label">ASSIGNED TO YOU</Typography>
              <Stack spacing={1} sx={{ mt: 1 }}>
                {assigned.map((task) => (
                  <Paper key={task.id} className="assigned-card">
                    <Stack direction="row" alignItems="center" justifyContent="space-between" sx={{ width: '100%' }}>
                      <Stack direction="row" spacing={2} alignItems="center">
                        <Box>
                          <Typography className="pixel-text" sx={{ color: '#fff', fontWeight: 700 }}>{task.title || 'Untitled'}</Typography>
                          <Stack direction="row" spacing={1} sx={{ mt: 1 }}>
                            <Chip label={`${task.reward ?? 20} XP`} size="small" className="chip-reward" style={{ background: '#fff700ff', color: '#000000ff', fontWeight: 700, boxShadow: '0 6px 18px rgba(59,7,16,0.08)' }} />
                            <Chip icon={<CalendarTodayIcon />} label={task.due_date ?? 'TODAY'} size="small" className="chip-date" style={{ background: '#05f2fac1', color: '#fff', fontWeight: 700, boxShadow: '0 6px 18px rgba(29,78,216,0.08)' }} />
                            {pinned.has(task.id) && <Chip label="PINNED" size="small" className="chip-pinned" style={{ background: '#5243e6', color: '#fff', fontWeight: 700, boxShadow: '0 6px 18px rgba(82,67,230,0.08)' }} />}
                          </Stack>
                        </Box>
                      </Stack>

                      <Stack direction="row" spacing={1} className="task-action-row">
                        <button className="button yellow" data-label={pinned.has(task.id) ? 'Unpin' : 'Pin'} onClick={() => togglePin(task.id)}>
                          <span className="svgIcon"><PushPinIcon fontSize="small" /></span>
                        </button>
                        <button className="button cyan" data-label="Edit" onClick={() => openEdit(task)}>
                          <span className="svgIcon"><EditIcon fontSize="small" /></span>
                        </button>
                        <button className="button red" data-label="Delete" onClick={() => console.log('delete task', task.id)}>
                          <span className="svgIcon"><DeleteIcon fontSize="small" /></span>
                        </button>
                      </Stack>
                    </Stack>
                  </Paper>
                ))}
              </Stack>
            </Box>

            <Divider sx={{ my: 2, borderColor: 'rgba(255,255,255,0.04)' }} />

            {/* Quest Queue */}
            <Box sx={{ p: 2 }}>
              <Typography className="section-label">Task Queue</Typography>
              <Stack spacing={1} sx={{ mt: 1 }}>
                {board.tasks.map((task, index) => (
                  <Paper key={task.id} className="assigned-card">
                    <Stack direction="row" alignItems="center" justifyContent="space-between" sx={{ width: '100%' }}>
                      <Stack direction="row" spacing={2} alignItems="center">
                        <Box>
                          <Typography className="pixel-text" sx={{ color: '#fff', fontWeight: 700 }}>{task.title || 'Untitled'}</Typography>
                          <Stack direction="row" spacing={1} sx={{ mt: 1 }}>
                            <Chip label={`${task.reward ?? 20} XP`} size="small" className="chip-reward" style={{ background: '#fff700ff', color: '#000000ff', fontWeight: 700, boxShadow: '0 6px 18px rgba(59,7,16,0.08)' }} />
                            <Chip icon={<CalendarTodayIcon />} label={task.due_date ?? 'TODAY'} size="small" className="chip-date" style={{ background: '#05f2fac1', color: '#fff', fontWeight: 700, boxShadow: '0 6px 18px rgba(29,78,216,0.08)' }} />
                            {pinned.has(task.id) && <Chip label="PINNED" size="small" className="chip-pinned" style={{ background: '#5243e6', color: '#fff', fontWeight: 700, boxShadow: '0 6px 18px rgba(82,67,230,0.08)' }} />}
                          </Stack>
                        </Box>
                      </Stack>

                      <Stack direction="row" spacing={1} className="task-action-row">
                        <button className="button yellow" data-label={pinned.has(task.id) ? 'Unpin' : 'Pin'} onClick={() => togglePin(task.id)}>
                          <span className="svgIcon"><PushPinIcon fontSize="small" /></span>
                        </button>
                        <button className="button cyan" data-label="Edit" onClick={() => openEdit(task)}>
                          <span className="svgIcon"><EditIcon fontSize="small" /></span>
                        </button>
                        <button className="button red" data-label="Delete" onClick={() => console.log('delete task', task.id)}>
                          <span className="svgIcon"><DeleteIcon fontSize="small" /></span>
                        </button>
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
                <TextField fullWidth label="Due date" sx={{ mt: 2 }} value={editingTask?.due_date ?? ''} onChange={(e) => setEditingTask({...editingTask, due_date: e.target.value})} />
                <FormControl fullWidth sx={{ mt: 2 }}>
                  <InputLabel id="assign-label">Assign to</InputLabel>
                  <Select labelId="assign-label" value={editingTask?.assignedTo ?? ''} label="Assign to" onChange={(e) => setEditingTask({...editingTask, assignedTo: e.target.value})}>
                    {/* mock options: use board members if available; fallback sample */}
                    <MenuItem value={editingTask?.assignedTo ?? ''}>{editingTask?.assignedTo ?? 'Unassigned'}</MenuItem>
                    <MenuItem value={'me'}>Me</MenuItem>
                    <MenuItem value={'someone'}>Someone</MenuItem>
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
                <TextField fullWidth label="Due date" sx={{ mt: 2 }} value={newTask.due_date} onChange={(e) => setNewTask({...newTask, due_date: e.target.value})} />
                <FormControl fullWidth sx={{ mt: 2 }}>
                  <InputLabel id="assign-label-add">Assign to</InputLabel>
                  <Select labelId="assign-label-add" value={newTask.assignedTo} label="Assign to" onChange={(e) => setNewTask({...newTask, assignedTo: e.target.value})}>
                    <MenuItem value={newTask.assignedTo ?? ''}>{newTask.assignedTo ?? 'Unassigned'}</MenuItem>
                    <MenuItem value={'me'}>Me</MenuItem>
                    <MenuItem value={'someone'}>Someone</MenuItem>
                  </Select>
                </FormControl>
              </DialogContent>
              <DialogActions>
                <Button onClick={() => setAddOpen(false)}>Cancel</Button>
                <Button onClick={() => { console.log('save add', newTask); setAddOpen(false); }} variant="contained">Create</Button>
              </DialogActions>
            </Dialog>
          </Paper>
          </div>
        </Container>
      </Box>
    )
  }