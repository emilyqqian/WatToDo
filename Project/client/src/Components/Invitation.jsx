import {
    Card,
    Typography,
    Stack,
    Avatar,
    ButtonBase,
    Box
} from '@mui/material'
import { useGlobal } from '../SessionManager'

export default function Invitation({ onClick }) {
    const { state, updateState } = useGlobal()

    return (
        <Card
            elevation={0}
            sx={{
                background: '#1a1f3a',
                backdropFilter: 'none',
                border: '3px solid rgba(255, 255, 255, 0.2)',
                borderRadius: '0px',
                p: 3,
                height: 'fit-content',
                top: '100px',
                mb: 3,
                boxShadow: '4px 4px 0px rgba(0, 0, 0, 0.15)',
            }}
        >
            <Stack spacing={2}>
                {/* Header */}
                <Box display="flex" alignItems="center" gap={1}>
                    <Typography variant="h6" fontWeight={700}>
                        Invitation
                    </Typography>
                </Box>

                {/* Leaderboard List */}
                {state.invitation.length == 0 ? (
                    <Box display="flex" justifyContent="center">
                        <Typography fontWeight={700}>
                            You have no pending invitation!
                        </Typography>
                    </Box>
                )
                 : 
                (
                    <Stack spacing={1.5}>
                        {state.invitation.map((invitation, index) => {
                            return (
                                <Box key={index}
                                    sx={{
                                    minWidth: '32px',
                                    height: '32px',
                                    display: 'flex',
                                    alignItems: 'center',
                                    justifyContent: 'center',
                                    fontWeight: 700,
                                    fontSize: '1.1rem',
                                }}
                                >
                                    <Avatar
                                        sx={{
                                            width: 32,
                                            height: 32,
                                            fontSize: '0.85rem',
                                            fontWeight: 'bold',
                                        }}
                                    >
                                        {invitation.inviter.username.charAt(0).toUpperCase()}
                                    </Avatar>
                                    
                                    <ButtonBase onClick={() => onClick(invitation)}>
                                        <Typography sx={{ml: '10px'}}>
                                            {invitation.inviter.username + " invites you to join " + invitation.taskboard.name}
                                        </Typography>  
                                    </ButtonBase>
                                </Box>
                            )
                        })}
                    </Stack>
                )}
            </Stack>
        </Card>
    )
}